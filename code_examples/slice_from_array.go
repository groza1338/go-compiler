package main

import "fmt"

func main() {
    arr := [4]int{7, 8, 9, 10}
    s := arr[1:3]
    fmt.Print(s[0])
    fmt.Print(" ")
    fmt.Print(s[1])
}
