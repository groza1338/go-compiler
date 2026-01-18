package main

import "fmt"

func main() {
    a := 3
    if a > 1 {
        fmt.Print("yes")
    } else {
        fmt.Print("no")
    }
    fmt.Print(" ")
    if a == 4 {
        fmt.Print("bad")
    }
}
