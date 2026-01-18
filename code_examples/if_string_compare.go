package main

import "fmt"

func main() {
    s := "ab"
    t := "ac"
    if s < t {
        fmt.Print("lt")
    }
    fmt.Print(" ")
    if s == "ab" {
        fmt.Print("eq")
    }
}
