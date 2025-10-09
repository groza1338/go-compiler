// mega_lexer_test.go
package main

import "fmt"
import (
	"os"
	"strings" // строка → ; (по правилу вставки перед NL)
)

const (
	_        = iota
	KiB int = 1 << (10 * iota) // сдвиг
	MiB
)

var (
	// целые
	d1 = 0
	d2 = 1_234_567
	o1 = 0o755        // новая запись восьмеричных
	o2 = 0755         // старая запись восьмеричных
	h1 = 0xDEAD_BEEF  // hex с '_'
	h2 = 0x42

	// float (только десятичные — без hexfloat)
	f1 = 3.1415
	f2 = .25
	f3 = 1e3
	f4 = 6.022_140_76e+23

	// руны (эскейпы: oct, hex, \u, \U, спец-символы)
	r1 rune = 'A'
	r2 rune = '\n'
	r3 rune = '\377'
	r4 rune = '\x7F'
	r5 rune = '\u0416'   // Ж
	r6 rune = '\U0001F60A' // 😊

	// строки
	s1 = "hi\t\"go\"" // интерпретируемая с эскейпами → ;
	s2 = `raw
text
with
newlines`            // raw с переводами → ;

	// композиты
	sl = []int{
		1,
		2, // хвостовая запятая — держит SEMI_OFF
	}
	m1 = map[int]string{
		1: "one",
		2: "two", // хвостовая запятая
	}
)

type point struct {
	X, Y int
	Tag  string `json:"tag,omitempty"` // raw-тэг
}

func sum(a ...int) int {
	// "..." — отдельный токен, SEMI_OFF
	if len(a) == 0 { // ')' → ;
		return 0
	}

	total := 0
	for i := 0; i < len(a); i++ { // явные ';' в заголовке for
		total += a[i] // ']' → ;
	}

	/* многострочный
	   комментарий с переводом строки:
	   предыдущая инструкция завершается как перед NL */
	total += 1 // → ;

	return total // return → ;
}

func pipes() {
	ch := make(chan int) // идентификатор перед NL → ;
	go func() {
		ch <- 1    // send + литерал → ;
		close(ch)  // идентификатор перед NL → ;
	}() // ')' → ;

	select {
	case v := <-ch:
		fmt.Println(v) // ')' → ;
	default:
	}
}

func labelsAndFlow() {
L: // метка: Identifier + ':'
	for i := 0; i < 2; i++ {
		if i == 0 {
			continue // → ;
		}
		break // → ;
	}
	goto L // идентификатор перед NL → ;
}

func ops() {
	u := 7      // → ;
	u &^= 1     // op-assign → ;
	w := u &^ 2 // '&^' оператор, держит SEMI_OFF внутри строки → ;
	_ = w       // → ;

	x := (1 +
		2) // строка заканчивается на ')' → ; (в грамматике ; перед ')' можно опустить)

	// dot-break: '.' в конце строки держит SEMI_OFF
	strings.
		NewReplacer("a", "b") // ')' → ;

	// индексация и срезы
	arr := [5]int{0, 1, 2, 3, 4}
	_ = arr[2]         // ']' → ;
	_ = arr[1:4]       // ']' → ;
	_ = arr[1:3:5]     // ']' → ;

	// составные присваивания и инкременты
	a := 1
	a += 2  // → ;
	a *= 3  // → ;
	a++     // '++' → ;
	a--     // '--' → ;

	// логика
	ok := (a > 0) && (s1 != "") // строка кончается на ')' → ;
	_ = ok

	// комментарии между элементами:
	var /* c1 */ f /* c2 */ = /* c3 */ 10
	_ = f // → ;

	// НЕЛЬЗЯ: перенос внутри интерпретируемой строки — должен сработать твой путь ошибки.
	// "broken
	// string"
}

func typesGenericsAndSwitch() {
	// простейшие дженерики — просто токены для лексера
	type Num interface{ ~int | ~int64 }
	type Pair[T any] struct{ A, B T }

	y := "hi"
	switch y {
	case "hi":      // строковый литерал → ;
		fallthrough // включает SEMI_ON → ;
	default:
	}

	var p Pair[int]
	_ = p

	// type switch (лексер видит '.' '(' 'type' ')')
	var anyv any = 10
	switch anyv.(type) {
	case int:
		fmt.Println("int") // ')' → ;
	default:
	}
}

func main() {
	fmt.Println(sum(1, 2, 3)) // ')' → ;
	pipes()                   // ')' → ;
	ops()                     // ')' → ;
	typesGenericsAndSwitch()  // ')' → ;
	_ = os.Getenv("X")        // ')' → ;
}
