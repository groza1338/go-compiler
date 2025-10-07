package main

import "fmt"

func main() {
    // Корректные примеры

    var r1 rune = 'a'
    var r2 rune = 'ä'
    var r3 rune = '\n'
    var r4 rune = '\t'
    var r5 rune = '\\'
    var r6 rune = '\''
    var r7 rune = '\u12e4'
    var r8 rune = '\U00101234'

    // Некорректные примеры

    var r9 rune = 'aa' // ошибка: слишком много символов
    var r10 rune = '\k' // ошибка: 'k' не является допустимым escape символом
    var r11 rune = '\xa' // ошибка: недостаточно цифр для escape \xHH
    var r12 rune = '\0' // ошибка: недостаточно цифр для escape \NNN
    var r13 rune = '\400' // ошибка: октальное значение вне диапазона
    var r14 rune = '\uDFFF' // ошибка: surrogate half, недопустимый код
    var r15 rune = '\U00110000' // ошибка: недопустимая кодовая точка Unicode
}
