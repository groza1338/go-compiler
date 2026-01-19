package main

import "fmt"

func main() {
    s := [][]int{[]int{1, 2}, []int{3, 4}}
    fmt.Print(s[0][1])
    fmt.Print(" ")
    fmt.Print(s[1][0])
}
