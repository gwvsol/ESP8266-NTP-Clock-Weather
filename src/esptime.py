import uasyncio as asyncio
from gc import collect
from time import time, localtime
from socket import socket, getaddrinfo, AF_INET, SOCK_DGRAM
from struct import unpack
from machine import RTC


try:
    from esputils import log
except ImportError as err:
    log = print
    log('ImportError: {}'.format(err))


    
class EspDateTime(object):
    """Класс для установки времени на микроконтроллере в 
       зависимости от временной зоны и перевода времени 
       на летнее и зимнее время"""
    """
import uasyncio as asyncio
from esptime import EspDateTime
zone = 3                # Moscow
daylight = False
tz = EspDateTime(zone, daylight)
await tz.setzone()
await tz.getlocaltime()
    """
    def __init__(self, zone:int = 0, daylight:bool = False, network:WiFiConnect = None, loop:bool = False):
        self.zone       = zone                  # Временная зона в которой работает микроконтроллер
        self.daylight        = daylight         # Учитывать перевод на летнее время
        # Поддержка временных зон
        self.time_zone  = {-11: -11, -10: -10, -9: -9, -8: -8, -7: -7, -6: -6, -5: -5, \
                           -4: -4, -3: -3, -2: -2, -1: -1, 0: 0, 1: 1, 2: 2, 3: 3, 4: 4, 5: 5, 6: 6, \
                            7: 7, 8: 8, 9: 9, 10: 10, 11: 11, 12: 12, 13: 13, 14: 14}
        # Месяцы в которых происходит перевод на летнее и зимнее время (для тех стран где это необходимо)
        self.month      = {'sum': 3, 'win': 10} # 3 - март, 10 - октябрь
        self.rtc        = RTC()                 # RTC микроконтроллера
        self.ntp_delta  = 3155673600
        self.ntp        = 0                     # Время с NTP сервера, начальное значение
        self.ntp_timeout = 3                    # TimeOut поучения информации с NTP сервера
        self.ntp_update  = 3600                 # Время обновления с NTP сервера - 86400
        self.ntp_next   = 0                     # Время обновления с NTP сервера - 86400
        self.host       = "pool.ntp.org"
        self.log        = log                   # Логирование
        self.network    = network
        self.collect    = collect
        if loop:
            self.loop = asyncio.get_event_loop()
            self.loop.create_task(self.updatetimeloop())

    
    async def updatetimeloop(self):
        """Метод для обновление времени микроконтроллера"""
        while True:
            try:
                if self.network.connect:
                    tm = time()
                    if self.ntp_next == 0 or tm >= self.ntp_next or tm <= self.ntp_next + 5:
                        await self.settime()
                        if self.ntp : 
                            self.ntp_next = tm + self.ntp_update
                            await asyncio.sleep(self.ntp_update)
                        else: 
                            self.ntp_next = tm + 10
                            await asyncio.sleep(4)
                else: await asyncio.sleep(4)
                await asyncio.sleep(1)
                self.collect()
            except AttributeError as err:
                self.log('EspDateTime => AttributeError', err)
                break
            await asyncio.sleep(0.01)


    def getntp(self) -> int:
        """Получение времени с NTP сервера"""
        self.log('EspDateTime => Get UTC time from NTP server...')
        NTP_QUERY = bytearray(48)   # Формируем запрос к NTP серверу
        NTP_QUERY[0] = 0x1b         # Формируем запрос к NTP серверу
        self.ntp = 0                # Сбрасываем предыдущее значение в 0

        try:
            addr = getaddrinfo(self.host, 123)[0][-1]
        except OSError as err:
            self.log('EspDateTime => OSError: {}'.format(err))
            return self.ntp
        s = socket(AF_INET, SOCK_DGRAM)
        s.settimeout(self.ntp_timeout)
        res = s.sendto(NTP_QUERY, addr)

        try:
            msg = s.recv(48)
        except OSError as err:
            self.log('EspDateTime => OSError: {}'.format(err))
            s.close()
            return self.ntp
        s.close()

        val = unpack("!I", msg[40:44])[0]
        self.ntp = val - self.ntp_delta

        ntp = localtime(self.ntp)
        year, month, day, hour, minute, second = ntp[0], ntp[1], ntp[2], ntp[3], ntp[4], ntp[5]
        self.log('EspDateTime <= NTP Time:', '{}-{}-{} {}:{}:{}'.format(year, month, day, hour, minute, second))

        return self.ntp


    async def sunday(self, year, month) -> int:
        """Вычисление последнего воскресенья в которое происходит
           перевод времени с летнего на зимнее время"""
        for d in range(1,32):
            a = (14 - month) // 12
            y = year - a
            m = month + 12 * a -2
            if (((d + y + y // 4 - y // 100 + y // 400 + (31 * m) // 12)) % 7) == 0:
                if d + 7 > 31: 
                    return d
        await asyncio.sleep(0.01)


    async def adj_tzone(self, utc:tuple) -> int:
        """Вычисление временной зоны"""
        summer = await self.sunday(utc[0], self.month.get('sum'))
        if utc[1] > self.month.get('sum'):
            summer = await self.sunday(utc[0], self.month.get('win'))
            if utc[1] <= self.month.get('win') and utc[2] < summer:
                rcv = self.time_zone.get(self.zone)
                self.log('EspDateTime => TIME ZONE Summer:', rcv)
        elif utc[1] == self.month.get('sum') and utc[2] >= summer:
            rcv = self.time_zone.get(self.zone)
            self.log('EspDateTime => TIME ZONE Summer:', rcv)
        else:
            if self.daylight: rcv = self.time_zone.get(self.zone) - 1
            else: rcv = self.time_zone.get(self.zone)
            self.log('EspDateTime => TIME ZONE Winter:', rcv)
        await asyncio.sleep(0.1)
        return rcv


    async def settime(self):
        """Метод для установки времени микроконтроллера"""
        ntp = self.getntp()
        if ntp:
            zone = await self.adj_tzone(localtime(ntp)) 
            newTime = localtime(ntp + (3600 * zone))
            self.log('EspDateTime => Update Time:', '{}-{}-{} {}:{}:{}'.format(newTime[0], \
                                                    newTime[1], newTime[2], newTime[3], newTime[4], newTime[5]))
            newTime = (newTime[0], newTime[1], newTime[2], newTime[6], newTime[3], newTime[4], newTime[5], 0)
            self.rtc.datetime(newTime)
        await asyncio.sleep(0.01)
        # local = localtime()
        # year, month, day, hour, minute, second = local[0], local[1], local[2], local[3], local[4], local[5]
        # self.log('EspDateTime => Local Time:', '{}-{}-{} {}:{}:{}'.format(year, month, day, hour, minute, second))

    
    async def gettime(self) -> tuple: # => (year, month, day, hour, minute, second)
        """Метод для получения локального времени"""
        await asyncio.sleep(0.1)
        local = localtime()
        # year, month, day, hour, minute, second = local[0], local[1], local[2], local[3], local[4], local[5]
        # self.log('EspDateTime => Local Time:', '{}-{}-{} {}:{}:{}'.format(year, month, day, hour, minute, second))
        return local

