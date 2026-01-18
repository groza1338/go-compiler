package main

import "fmt"

func avg(a float64, b float64) float64 {
    return (a + b) / 2.0
}

func main() {
    fmt.Print(avg(3.0, 5.0))
}
