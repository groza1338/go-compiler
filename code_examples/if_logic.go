package main

import "fmt"

func main() {
    a := true
    b := false
    if a && b {
        fmt.Print("bad")
    } else {
        fmt.Print("ok")
    }
    fmt.Print(" ")
    if a || b {
        fmt.Print("yes")
    }
}
