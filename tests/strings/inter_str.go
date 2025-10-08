package main

import "fmt"

func main() {
    // Примеры строковых литералов с допустимыми escape-последовательностями
    str1 := "Hello, World!"        // Простая строка
    str2 := "Line1\nLine2"          // Строка с переносом строки
    str3 := "Tab\tIndented"        // Строка с табуляцией
    str4 := "Quote: \"Quoted\""    // Строка с экранированной кавычкой
    str5 := "Backslash: \\"        // Строка с экранированной обратной косой чертой
    str6 := "\u0041 is A"          // Строка с Unicode escape (U+0041 -> 'A')
    str7 := "Byte: \x41"           // Строка с байтовым значением (ASCII 'A')
    str8 := "\U0001F600 is Grinning Face" // Строка с Unicode escape (U+1F600 -> Grinning Face)

    str9 := "Invalid Unicode \uD800" // Некорректный Unicode escape (surrogate half)
    str10 := "Bad byte value \xG1"  // Некорректный байтовый escape (невалидный символ после \x)
    str11 := "Too few octal digits \377" // Некорректный октальный escape (недостаточно цифр)
}
