package main

import "fmt"

func addHalf(x float64) float64 {
    return x + 0.5
}

func main() {
    fmt.Print(addHalf(1.5))
}
