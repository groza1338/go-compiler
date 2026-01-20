package main

import "fmt"

func main() {
    var n int
    fmt.Print("Enter number: ")
    fmt.Scan(&n)

    switch n {
        case 10, 20, 30:
            fmt.Print("ten or twenty or thirty")
        case -1:
            fmt.Print("minus one")
        case 0:
            fmt.Print("zero")
        case 1:
            fmt.Print("one")
        default:
            fmt.Print("other ")
    }

    switch {
        case n > 100:
            fmt.Print("> 100")
        default:
            fmt.Print("абоба")
    }
}
