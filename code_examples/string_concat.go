package main

import "fmt"

func main() {
    s := "hi"
    s += " there"
    t := s + "!"
    fmt.Print(t)
    fmt.Print(" ")
    u := "foo" + "bar"
    fmt.Print(u)
}
