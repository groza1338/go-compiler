package main

import "fmt"

func main() {
	/*
		Multi-line comment before assignment
	*/

	var decimal = 1_000_000
    var octal = 0o755
    var hex = 0xFFEE
    var floatSimple = 12.5
    var floatExp = 6.022e23
    var floatLeading = .25
    var newline rune = '\n'

    var interpreted = "Line1\nLine2\tTabbed \"quote\""
    var unicodeText = "Snowman: \u2603"
    var raw = `Raw string keeps \n literal
    and spans two lines.`

    x, y := 1, 3
    a, b, c := 10, 20, 30

    const name string = "demo"
    const number int64 = 10
    const ratio float64 = 1.5

    b := (2 + 3) * 4
    c := 10 - 2*3 + 1
    d := 10 - (2*3 + 1)
    h := (2 + 3) * (4 + 5)
    logical := (a > b) && (c < d || e == f)

    var values [2]int
    var b [2]int

    values[0] = 10
    values[1] = values[0] - 1

    var grid [2][3]int

    grid[0][1] = 4
    grid[1][2] = grid[0][1] + 1

    if value > 5 {
        _ = value
    } else {
        _ = value - 1
    }

    if value > 5 {
        _ = value
    } else if value == 0 {
        _ = value - 1
    } else {
        _ = 5
    }

    if temp := value - 1; temp == 2 {
        _ = temp
    }

    for i := 0; i < 5; i++ {
        if i == 0 {
            continue
        }

        count = count + i

        if count > 5 {
            break
        }
    }

    fmt.Scan(&x)
    fmt.Println(x)
}

func pick(value int64) int64 {
	switch value {
	case 0:
		return value
	case 1:
		return value + 1
	default:
		return value + 2
	}
}
