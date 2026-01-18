package main

import "fmt"

func twice(x int) int {
    return x * 2
}

func add(a int, b int) int {
    return a + b
}

func main() {
    fmt.Print(twice(add(1, 2)))
}
