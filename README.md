# Notar Language (NTR)

**Notar Language (NTR)** is een eenvoudige, dynamische programmeertaal met ondersteuning voor:

- Dynamische variabelen
- Constante en typevaste variabelen
- Functies
- Arrays
- Objects
- If-statements
- While-loops
- Referenties
- Basis typecontrole
- Automatische typeconversie

---

# Hello World

Het kleinste mogelijke NTR-programma is:

```ntr
__out("Hello World");
```

__out() schrijft direct naar de uitvoer. De functie converteert automatisch alle waarden naar tekst, waardoor je geen aparte conversie nodig hebt.

In de praktijk is het vaak handiger om een eigen print() functie te maken:

```ntr
func print(text){
    __out(text, "\n");
}
```
Gebruik:

```ntr
print("Hello World");
```
Omdat __out() automatisch waarden naar tekst converteert, kunnen ook andere typen direct worden meegegeven, zoals:

Numbers
Booleans
Arrays
Objects

---

# Variabelen

Variabelen worden aangemaakt met het `var` keyword.

```ntr
var x = 10;
```

NTR is standaard dynamisch getypeerd. Zowel de waarde als het type mogen veranderen.

```ntr
var x = 10;

x = 20;
x = "Hello";
x = true;
```

---

# Modifiers

NTR ondersteunt twee modifiers voor variabelen.

## `fix`

Een `fix` variabele is constant. Nadat deze is aangemaakt kan de waarde niet meer worden gewijzigd.

```ntr
var pi: fix = 3.14159;
```

Toegestaan:

```ntr
print(pi);
```

Niet toegestaan:

```ntr
pi = 5;
```

---

## `pure`

Een `pure` variabele heeft een vast type, maar de waarde mag veranderen.

```ntr
var score: pure = 10;
```

Toegestaan:

```ntr
score = 25;
```

Niet toegestaan:

```ntr
score = "high";
```

Omdat `score` oorspronkelijk een `number` was.

---

# Datatypes

NTR ondersteunt onder andere:

- Numbers
- Strings
- Booleans
- Arrays
- Objects

---

# Arrays

Arrays bevatten meerdere waarden.

```ntr
var numbers = [1, 2, 3];
```

Elementen worden benaderd met een index.

```ntr
print(numbers[0]);
```

Uitvoer:

```text
1
```

Een element aanpassen:

```ntr
numbers[1] = 20;
```

Arrays mogen verschillende typen bevatten.

```ntr
var values = [
    10,
    "Hello",
    true
];
```

---

# Objects

Objects slaan gegevens op als sleutel-waardeparen.

```ntr
var player = {
    name: "Bob",
    score: 10
};
```

Properties worden benaderd met een punt.

```ntr
print(player.name);
print(player.score);
```

Properties kunnen ook worden aangepast.

```ntr
player.score = 25;
```

---

# Complexe datastructuren

Arrays en objects kunnen vrij gecombineerd worden.

```ntr
var game = {
    board: [
        " ", " ", " ",
        " ", "X", " ",
        " ", " ", " "
    ],

    turn: "O"
};
```

Waarden uitlezen:

```ntr
game.board[4];
game.turn;
```

Waarden aanpassen:

```ntr
game.board[4] = "O";
game.turn = "X";
```

---

# Operatoren

## Rekenkundige operatoren

NTR ondersteunt de volgende operatoren:

- `+`
- `-`
- `*`
- `/`

Voorbeeld:

```ntr
var x = 10;

x = x + 5;
```

---

## Verkorte assignments

Beschikbaar:

- `+=`
- `-=`
- `*=`
- `/=`

Voorbeeld:

```ntr
var x = 10;

x += 5;
x -= 2;
x *= 3;
x /= 2;
```

---

## Vergelijkingen

Ondersteunde vergelijkingen:

- `==`
- `!=`
- `<`
- `>`
- `<=`
- `>=`

Voorbeeld:

```ntr
if(x == 10){
    print("Ten");
}
```

---

## Logische operatoren

Beschikbaar:

- `&&`
- `||`

Voorbeeld:

```ntr
if(x > 0 && x < 10){
    print("Between");
}
```

---

# Functies

Functies worden gemaakt met het `func` keyword.

```ntr
func add(a, b){
    return a + b;
}
```

Aanroepen:

```ntr
var result = add(5, 10);
```

Functies kunnen een waarde teruggeven.

```ntr
func square(x){
    return x * x;
}
```

---

# If-statements

```ntr
if(x == 10){
    print("Yes");
}
else{
    print("No");
}
```

---

# While-loops

Een `while`-loop wordt uitgevoerd zolang de conditie waar blijft.

```ntr
var i = 0;

while(i < 10){
    print(i);
    i += 1;
}
```

---

# Referenties (`&`)

Normaal worden waarden als kopie doorgegeven.

Met `&` geef je een referentie naar een bestaande variabele door.

```ntr
var value = 10;

func setZero(x){
    x = 0;
}

setZero(&value);
```

Omdat de functie een referentie ontvangt, wordt de originele variabele aangepast.

Een praktisch voorbeeld is invoer lezen.

Normaal:

```ntr
var input = __cast(0, __in());
```

Of rechtstreeks naar een bestaande variabele:

```ntr
var input = 0;

__in(&input);
```

Referenties zijn vooral handig bij grote arrays of objects, of wanneer een functie direct een bestaande variabele moet wijzigen.

---

# Built-in functies

## `__out()`

Schrijft gegevens naar de uitvoer.

```ntr
__out("Hello");
```

Meerdere waarden tegelijk:

```ntr
__out("Score: ", score);
```

---

## `__in()`

Leest invoer van de gebruiker.

Standaard geeft deze een string terug.

```ntr
var text = __in();
```

Een number lezen:

```ntr
var number = __cast(0, __in());
```

Of direct naar een bestaande variabele schrijven:

```ntr
var number = 0;

__in(&number);
```

---

## `__cast()`

Converteert een waarde naar het type van het eerste argument.

```ntr
var number = __cast(0, "25");
```

Of naar een string:

```ntr
var text = __cast("", 123);
```

---

# Voorbeeld

```ntr
func print(text){
    __out(__cast("", text), "\n");
}

var player = {
    name: "Alice",
    health: 100
};

while(player.health > 0){

    print(player.name);

    player.health -= 10;
}
```
---

# Tic tac toe

Je kunt de tic_tac_toe game uitproberen door het main programma te runnen, je kunt ook zelf files aanmaken en schrijven, als je die wilt runnen doe je dat door in main.cpp de file die wordt doorgegevenn te veranderen naar jouw eigen file. 
Standaard staat debug mode uit, maar je kunt ook in main.cpp bij Run_program() true doorgeven om debug mode aan te zetten.

---

# Samenvatting

| Onderdeel | Syntax |
|-----------|--------|
| Variabele | `var x = value;` |
| Constante | `var x: fix = value;` |
| Vast type | `var x: pure = value;` |
| Functie | `func name(args){}` |
| Return | `return value;` |
| Array | `[1,2,3]` |
| Object | `{ key: value }` |
| Property | `object.key` |
| Index | `array[index]` |
| Referentie | `&variable` |
| If | `if(condition){}` |
| While | `while(condition){}` |

---

# Over NTR

NTR biedt een compacte maar krachtige basis voor het bouwen van programma's.

Ondanks de eenvoudige syntax ondersteunt de taal:

- Functies
- Scopes
- Variabelen met modifiers
- Arrays
- Objects
- Referenties
- Operatoren
- Typecontrole
- Automatische conversies via ingebouwde functies