package main

import "fmt"

func main() {
    s := "b"
    switch s {
    case "a":
        fmt.Print("A")
    case "b":
        fmt.Print("B")
    default:
        fmt.Print("Z")
    }
}
