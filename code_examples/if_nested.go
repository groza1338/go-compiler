package main

import "fmt"

func main() {
    a := 2
    if a > 0 {
        if a == 2 {
            fmt.Print("inner")
        } else {
            fmt.Print("inner_else")
        }
    } else {
        fmt.Print("outer_else")
    }
}
