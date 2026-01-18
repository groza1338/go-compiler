package main

import "fmt"

func main() {
    var count int
    var price float64
    var ok bool
    var name string
    fmt.Scan(&count, &price, &ok, &name)
    fmt.Print(count, " ", price, " ", ok, " ", name)
}
