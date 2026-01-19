package main

import "fmt"

func main() {
    var f float64
    f = 3
    fmt.Print(f)
    fmt.Print(" ")
    arr := [...]float64{1.1, 2.2, 3.3}
    arr[1] = 4
    fmt.Print(arr[0])
    fmt.Print(" ")
    fmt.Print(arr[1])
}
