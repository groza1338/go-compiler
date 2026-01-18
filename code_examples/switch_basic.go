package main

import "fmt"

func main() {
    x := 2
    switch x {
    case 1:
        fmt.Print("one")
    case 2:
        fmt.Print("two")
    default:
        fmt.Print("other")
    }
}
