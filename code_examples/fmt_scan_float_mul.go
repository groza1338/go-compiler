package main

import "fmt"

func main() {
    var x float64
    var y float64
    fmt.Scan(&x, &y)
    prod := x * y
    fmt.Print(prod)
}
