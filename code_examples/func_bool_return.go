package main

import "fmt"

func isPositive(x int) bool {
    return x > 0
}

func main() {
    fmt.Print(isPositive(2))
    fmt.Print(" ")
    fmt.Print(isPositive(-1))
}
