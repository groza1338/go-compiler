package main

import "fmt"

func main() {
    s := []int{4, 5, 6}
    s[1] = 10
    fmt.Print(s[0])
    fmt.Print(" ")
    fmt.Print(s[1])
}
