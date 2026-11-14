# Syntax

| Onderdeel   | Syntax                               |
|-------------|--------------------------------------|
| Variabele   | `var x = value;`                     |
| Constante   | `var x: fix = value;`                |
| Type vast   | `var x: pure = value;`               |
| Functie     | `func name(args){}`                  |
| Return      | `return value;`                      |
| Break       | `Break;`                             |
| Continue    | `continue;`                          |
| Array       | `[1,2,3]`                            |
| Object      | `{ key: value }`                     |
| Property    | `object.key`                         |
| Index       | `array[index]`                       |
| Referentie  | `&variable`                          |
| If          | `if(condition){}`                    |
| Else if     | `elif(condition){}`                  |
| Else        | `else{}`                             |
| While       | `while(condition){}`                 |
| For         | `for(var it; condition; operation){}`|

Zoals je ziet lijkt de syntax een beetje op een mix van python en js.

---

# Builtin functies

| Functie     | Argumenten        | Syntax |
|-------------|-------------------|--------|
| `__out()`     | alle args worden achter elkaar geprint. |Wordt gebruikt om output te printen naar de terminal. Alle data types worden automatisch gecast naar een string.|
| `__in()`      | alle args worden in een prompt gebruikt. Als er een referentie in zit wordt vanaf die referentie alles toegewezen. |Wordt gebruikt om input te krijgen van de user. Geeft de input terug als string, of kan het assignen aan een referentie.|
| `__cast()`    | het eerste argument is een waarde van het type waarnaar je wilt casten. De volgende waardes worden allemaal gecast naar dat type en terug gegeven in een array. |Wordt gebruikt om types te converten naar andere types indien dat mogelijk is. Als het niet mogelijk is geeft het een error value terug.|
| `__failed()`  | alle args worden gechecked op Error value en als er 1 error is dan returned en true, zo niet dan false. |Wordt gebruikt om te checken of een waarde een error value is. Geeft een boolean terug.|

