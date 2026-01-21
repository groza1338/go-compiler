package main

import "fmt"

func main() {
	const (
		a = iota
		b
		_
		c = iota + 2
		d
	)

	const (
		_ = iota
		e = iota * 2
		f
	)

	const (
		g int = iota
		h
	)

	const (
		i = 1 + iota
		j = i + iota
		k = j + iota
	)

	const (
		l = iota + 10
		m = iota - 1
		n = (iota + 1) * 2
	)

	fmt.Println(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
}
