package main

import "fmt"

func stats(x int) (int, bool) {
    return x + 1, x > 0
}

func main() {
    v, ok := stats(1)
    fmt.Print(v)
    fmt.Print(" ")
    fmt.Print(ok)
}
