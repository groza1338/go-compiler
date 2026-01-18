package main

import "fmt"

func main() {
    x := 3
    switch {
    case x < 2:
        fmt.Print("lt")
    case x == 3:
        fmt.Print("eq")
    default:
        fmt.Print("gt")
    }
}
