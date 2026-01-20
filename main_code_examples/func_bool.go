package main

import "fmt"

func toggle(x bool) bool {
    return !x
}

func main() {
    fmt.Print(toggle(true))
}
