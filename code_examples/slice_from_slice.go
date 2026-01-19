package main

import "fmt"

func main() {
    s := []int{1, 2, 3, 4}
    t := s[2:]
    fmt.Print(t[0])
    fmt.Print(" ")
    fmt.Print(t[1])
}
