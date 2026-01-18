package main

import "fmt"

func main() {
    a := 5
    b := 7
    fmt.Print(a < b)
    fmt.Print(" ")
    fmt.Print(a == b)
    fmt.Print(" ")
    fmt.Print(a != b)
    fmt.Print(" ")

    x := 2.0
    y := 2.0
    fmt.Print(x >= y)
    fmt.Print(" ")

    s := "ab"
    t := "ac"
    fmt.Print(s < t)
    fmt.Print(" ")
    fmt.Print(s == "ab")
    fmt.Print(" ")

    fmt.Print(true && false)
    fmt.Print(" ")
    fmt.Print(true || false)
    fmt.Print(" ")
    fmt.Print(!false)
}
