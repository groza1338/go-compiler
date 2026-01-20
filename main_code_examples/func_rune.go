package main

import "fmt"

func incRune(r rune) rune {
    return r + 1
}

func main() {
    fmt.Print('a')
    fmt.Print(" ")
    fmt.Print(incRune('a'))
}
