# scheduling-engine

## 1. **Example 1**

### 1.0. Conflict Relationship
|   | 0 | 1 | 2 | 3 | 4 |
|---|---|---|---|---|---|
| 0 | &check; | &check; | &check;  | &check;  | &check;  |
| 1 | &check; | &check; | **&cross;**  | **&cross;**  | &check;  |
| 2 | &check; | &check; | &check;  | &check;  | **&cross;**  |
| 3 | &check; | &check; | &check;  | &check;  | **&cross;**  |
| 4 | &check; | &check; | &check;  | &check;  | &check;  |




### 1.1. Execution schedule
| TX ID  | Callee ID  | Output Tx Order  | Output Branch | Output Generation|
|---|---|---|---|---|
|  7  |  0  | 7 | 0 | 0  |
|  6  |  0  | 6 | 1 | 0  |
|  5  |  0  | 5 | 2 | 0  | 
|  4  |  1  | 4 | 3 | 0  | 
|  3  |  2  | 0 | 4 | 0  | 
|  2  |  2  | 3 | 0 | 1  |
|  1  |  3  | 2 | 1 | 1  |  
|  0  |  4  | 1 | 2 | 1  |  



## **Example 2**

### 2.0 Conflict Relationship

|   | 0 | 1 | 2 | 3 | 4 |
|---|---|---|---|---|---|
| 0 | **&cross;** | &check; | &check;  | &check;  | &check;  |
| 1 | &check; | &check; | &check;  | &check;  | &check;  |
| 2 | &check; | &check; | **&cross;**  | &check;  | &check;  |
| 3 | &check; | &check; | &check;  | &check;  | &check;  |
| 4 | &check; | &check; | &check;  | &check;  | &check;  |


### 2.1 Execution schedule
| TX ID  | Callee ID  | Output Tx Order  | Output Branch | Output Generation|
|---|---|---|---|---|
|  7  |  0  | 4 | 0 | 0  |
|  6  |  0  | 1 | 1 | 0  |
|  5  |  0  | 0 | 2 | 0  | 
|  4  |  1  | 7 | 0 | 1  | 
|  3  |  2  | 6 | 0 | 1  | 
|  2  |  2  | 5 | 0 | 1  |
|  1  |  3  | 3 | 1 | 1  |  
|  0  |  4  | 2 | 1 | 1  |  
