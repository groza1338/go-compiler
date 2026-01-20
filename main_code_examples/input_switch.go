package main

import "fmt"

func main() {
    var n int
    fmt.Print("Enter number: ")
    fmt.Scan(&n)

    switch n {
        case -1:
            fmt.Print("minus one")
        case 0:
            fmt.Print("zero")
        case 1:
            fmt.Print("one")
        default:
            fmt.Print("other")
    }
}
