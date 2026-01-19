package main

import "fmt"

func main() {
    s := [][]int{[]int{1, 2}, []int{3, 4}}
    s[0][0] = 7
    fmt.Print(s[0][0])
    fmt.Print(" ")
    fmt.Print(s[1][1])
}
