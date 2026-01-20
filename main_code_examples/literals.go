package main

import "fmt"

func main() {
	// Integer literals
	fmt.Println("DEC:", 42)
	fmt.Println("OCT 0o755:", 0o755)
	fmt.Println("OCT 0755:", 0755)
	fmt.Println("HEX 0x2A:", 0x2A)
	fmt.Println("SEP 1_000_000:", 1_000_000)
	fmt.Println("UNARY -42:", -42)

	fmt.Println()

	// Rune literals
	fmt.Println("RUNE A:", 'A')
	fmt.Println("RUNE Ж:", 'Ж')
	fmt.Println("ESC \\n:", '\n')
	fmt.Println("ESC \\t:", '\t')
	fmt.Println("ESC \\r:", '\r')
	fmt.Println("ESC \\a:", '\a')
	fmt.Println("ESC \\b:", '\b')
	fmt.Println("ESC \\v:", '\v')
	fmt.Println(`ESC \':`, '\'')
	fmt.Println(`ESC \" :`, '"')
	fmt.Println("ESC \\\\:", '\\')
	fmt.Println("ESC \\377:", '\377')
	fmt.Println("ESC \\x7F:", '\x7F')
	fmt.Println("ESC \\u0416:", '\u0416')
	fmt.Println("ESC \\U0001F680:", '\U0001F680')

	fmt.Println()

	// Strings
	fmt.Println("STR interp:", "строка\n\t\"кавычки\"")
	fmt.Println("STR raw:", `как есть \n без экранирования многострочно`)
}
