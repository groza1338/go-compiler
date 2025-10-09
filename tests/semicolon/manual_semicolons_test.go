package main

import "fmt"

func main() {
	// 1) Несколько выражений в одну строку — явные ';' между stmt
	a := 1; b := 2; c := a + b; fmt.Println(c) // ожидаем 3 токена 'Operator: ;' внутри строки

	// 2) Полная ForClause — два обязательных ';' в заголовке (самостоятельные токены)
	for i := 0; i < 3; i++ { fmt.Println(i); } // явный ';' перед '}', всё на одной строке

	// 3) ForClause без условия — ';' в заголовке всё равно ОБЯЗАТЕЛЬНЫ
	for i := 0; ; i++ { break }                // перед '}' точку с запятой можно опустить (см. правило (2))

	// 4) Пустые init/post, явные ';' и несколько stmt в строке внутри тела цикла
	var j int
	for ; j < 3 ; { j++; if j == 2 { continue }; fmt.Println(j) }

	// 5) if с init-stmt — обязательный ';' в заголовке; далее два stmt в одну строку
	if x := a + b; x > 0 { fmt.Println(x) }; fmt.Println("done")

	// 6) switch с init-stmt — обязательный ';' в заголовке; case на одной строке
	switch y := c % 2; y {
	case 0: fmt.Println("even"); default: fmt.Println("odd");
	}

	// 7) Несколько простых stmt на одной линии вне блоков
	i := 0; i++; i += 2; fmt.Println(i)

		// --- if с ПУСТЫМ init-stmt: "if ; cond { ... }"
	x := 1
	if ; x > 0 {
		fmt.Println("if-empty-init")
	}

	// --- switch с ПУСТЫМ init-stmt и обычным tag-выражением
	y := 2
	switch ; y % 2 {
	case 0:
		fmt.Println("even")
	default:
		fmt.Println("odd")
	}

	// --- type switch с ПУСТЫМ init-stmt и guard без ":="
	var anyv any = 10
	switch ; anyv.(type) {
	case int:
		fmt.Println("type-only-guard")
	default:
	}

	// --- type switch с ПУСТЫМ init-stmt и guard C ":="
	switch ; v := anyv.(type) {
	case int:
		_ = v
	default:
	}

	// --- switch с ПУСТЫМ init-stmt и канал-получением как tag-выражением
	ch := make(chan int)
	go func() { ch <- 1 }()
	switch ; <-ch {
	case 1:
		fmt.Println("one")
	default:
	}

	// --- select без init-stmt (у select его не бывает) — просто базовый кейс
	select {}

	if 1 > 0 { fmt.Println("a") } else { fmt.Println("b") }

	if 1 > 0 { fmt.Println("a") }
	else { fmt.Println("b") }
	return 42
	x



}
