package main

import "fmt"

func inc(x int) int {
    return x + 1
}

func sum(a int, b int) int {
    return a + b
}

func main() {
    fmt.Print(inc(sum(2, 3)))
}
