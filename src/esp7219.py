from machine import Pin, SPI
from micropython import const
import framebuf
from time import localtime, sleep

"""14 регистров, с помощью которых осуществляется управление драйвером. 
При задании адреса старший полубайт не имеет значения"""

_NoOp = const(0x00)                             # Регистр No-Op используется для обращения к конкретному драйверу 
                                                # при использовании каскадного подключения нескольких драйверов

_DIGIT_0 = const(0x01)                          # Данные для 0-го индикатора
_DIGIT_1 = const(0x02)                          # Данные для 1-го индикатора
_DIGIT_2 = const(0x03)                          # Данные для 2-го индикатора
_DIGIT_3 = const(0x04)                          # Данные для 3-го индикатора
_DIGIT_4 = const(0x05)                          # Данные для 4-го индикатора
_DIGIT_5 = const(0x06)                          # Данные для 5-го индикатора
_DIGIT_6 = const(0x07)                          # Данные для 6-го индикатора
_DIGIT_7 = const(0x08)                          # Данные для 7-го индикатора

_DECODE_MODE = const(0x09)                      # Регистр управления режимом декодирования данных
_NO_DECODE = const(0x0)

_INTENSITY = const(0xA)                         # Регистр настройки яркости свечения индикаторов
                                                # Изменение яркости свечения реализовано с помощью ШИМ, 
                                                # всего имеется 16 уровней яркости от 0x00 до 0x0F
                                                # Значение старшего полубайта регистра Intensity не имеет значения
                                                # Максимальная яркость ограничивается резистором на выводе ISET
_INTENSITY_MIN = const(0x00)
_INTENSITY_MAX = const(0x0F)

_SCAN_LIMIT = const(0xB)                        # Регистр настройки количества активных индикаторов
_DISPLAY_ALL_DIGITS = const(0x07)               # При 0x00 - активных индикатор только 0, 
                                                # при 0x07 - активные индикаторы 0,1,2,3,4,5,6,7
                                                # Значение битов 3-7 в регистре Scan Limit не имеет значения
                                                # От количества активных индикаторов зависит частота их обновления

_SHUTDOWN = const(0xC)                          # Регистр управления режимом работы
                                                # В регистре Shutdown активным является только 0-й бит, 
                                                # если значение этого бита установить в 0, то драйвер перейдет в 
                                                # спящий режим, и отключит индикаторы, при этом содержимое регистров 
                                                # не изменяется, при установке бита в 1 драйвер просыпается
_SHUTDOWN_MODE = const(0x00)
_NORMAL_OPERATION = const(0x01)

_DISPLAY_TEST = const(0xF)                      # Регистр для тестирования драйвера
                                                # С помощью регистра Display Test можно проверить работоспособность 
                                                # сегментов на индикаторах, для этого необходимо установить 0-й бит 
                                                # регистра в 1, после чего засветятся все сегменты на всех индикаторах, 
                                                # для выключения теста надо установить бит обратно в 0
_DISPLAY_TEST_NORMAL_OPERATION = const(0x00)

_MATRIX_SIZE = const(8)

###########################################


class Max7219(framebuf.FrameBuffer):
    """Драйвер для MAX7219 8x8 LED матрицы"""
    def __init__(self, width, height, spi, cs):
        # Настройка выводов к которым подключена LED матрица
        self.spi = spi              # Настройка SPI порта
        self.cs = cs                # Настройка вывода Cable Select
        self.cs.init(Pin.OUT, True) # Настройка вывода Cable Select

        # Настройка размеров экрана в пикселях
        self.width = width          # Ширина
        self.height = height        # Высота
        # Расчет расположения LED матриц
        self.cols = width // _MATRIX_SIZE           # Количество колонок матриц
        self.rows = height // _MATRIX_SIZE          # Количество строк матриц
        self.nb_matrices = self.cols * self.rows    # Подсчет количества LED 

        # Формируем буфер для отправки данных на драйвер/драйверы матриц
        # 1 bit равен одному пикселю (on/off)
        # В одной матрице 8 байт
        self.buffer = bytearray(self.width * self.height // 8)
        # Установка формата вывода на матрицу в зависимости от необходимости
        # переворота вывода на 180 градусов
        # http://docs.micropython.org/en/latest/library/framebuf.html#constants
        self.format = framebuf.MONO_HLSB
        super().__init__(self.buffer, self.width, self.height, self.format)

        # Инициализация LED матриц
        self.init_display()

    
    def byte_to_str(self, data:bytearray) -> str:
        """Метод для перевода bytes bytearray или list в строку"""
        return ' '.join(["{:08b}".format(i) for i in data])

    
    def _write_command(self, command, data):
        """Метод для записи данных по SPI """
        cmd = bytearray([command, data])
        self.cs(0)
        for matrix in range(self.nb_matrices):
            self.spi.write(cmd)
        self.cs(1)


    def init_display(self):
        """Метод для инициализации LED матриц"""
        for command, data in (
                (_SHUTDOWN, _SHUTDOWN_MODE),        # Предотвращение вспышки во время инициализации
                (_DECODE_MODE, _NO_DECODE),         # Выключаем декодирование данных драйвером
                (_DISPLAY_TEST, _DISPLAY_TEST_NORMAL_OPERATION),
                (_INTENSITY, _INTENSITY_MIN),       # Включаем минимальную интенсивность свечения индикатора
                (_SCAN_LIMIT, _DISPLAY_ALL_DIGITS), # Включаем работу всех 8 индикаторов
                (_SHUTDOWN, _NORMAL_OPERATION),     # Завершение инициализации и перевод драйвера в нормальный режим работы
        ):
            self._write_command(command, data)

        self.fill(0)
        self.show()


    def brightness(self, value):
        """Метод для установки яркости свечения LED мариц (0 to 15)"""
        if not 0 <= value < 16:
            raise ValueError('Brightness must be between 0 and 15')
        self._write_command(_INTENSITY, value)


    def show(self, clock=True, sep=False):
        """Метод для обновления LED матриц"""
        # Записываем данные строка за строкой
        
        for line in range(8):
            self.cs(0)
            data = bytearray()
            for matrix in range(self.nb_matrices):
                dig = bytearray()
                dig.append(_DIGIT_0 + line)
                # Вычисляем нахождение LED матрицы
                row, col = divmod(matrix, self.cols)
                # Вычисляем где начинаются данные
                offset = row * 8 * self.cols
                index = col + line * self.cols + offset
                buff = self.buffer[index]
                # Преобразование только для вывода часов
                line_sep = [1, 2, 4, 5] # На линиях 1,2,4,5 и индикаторах 1 и 2 отображаем точки разделители
                if matrix == 1 and clock: # На первом индикаторе смещаем цифру на 1 пиксель влево
                    buff = self.buffer[index] << 1
                    if line in line_sep and sep: buff = buff | 1
                if matrix == 2 and clock: # На втором индикаторе смещаем цифру на 1 пиксель вправо
                    buff = self.buffer[index] >> 1
                    if line in line_sep and sep: buff = buff | 128
                dig.append(buff)
                data.extend(dig)
            self.spi.write(data)
            self.cs(1)


class ClockMax7219(object):
    """
from esp7219 import ClockMax7219
clock = ClockMax7219()
clock.clock()
    """

    def __init__(self):
        self.spi = SPI(1, baudrate=20000000)
        self.screen = Max7219(32, 8, self.spi, Pin(15))


    def show_clock(self, sep=True):
        self.screen.fill(0)
        self.screen.show()
        local = localtime()
        year, month, day, hour, minute, second = local[0], local[1], local[2], local[3], local[4], local[5]
        hour = str(hour) if hour >= 10 else '0{}'.format(hour)
        minute = str(minute) if minute >= 10 else '0{}'.format(minute)
        clock = '{}{}'.format(hour, minute)
        self.screen.text(clock, 0, 0, 1)
        self.screen.show(sep=sep)

    
    def clock(self):
        sep = True
        while True:
            self.show_clock(sep=sep)
            sep = False if sep else True
            sleep(3)
            
            