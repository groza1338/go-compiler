package main

import "fmt"

func duo() (int, string) {
    return 7, "ok"
}

func main() {
    a, b := duo()
    fmt.Print(a)
    fmt.Print(" ")
    fmt.Print(b)
}
