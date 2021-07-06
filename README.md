# Basic-Backtesting  
Performed basic backtesting on 1 minute Nifty data between 26 October 2020 and 18 January 2021  
## Strategy:  
See previous day's high price. When prices of current day cross the high of previous day give a "Buy" signal, and buy at that price. This will result in the trade status being "active". If there is no Buy signal, the trade status will be "inactive".  
Target at which to sell will be Buy price, plus the difference of previous day high and low (Buy price + (Previous day high - Previous Day low)).  
Maximum loss (Stop Loss) at which to sell will be Buy price, minus one fourth of the difference of previous day high and low (Buy price - 1/4* (Previous day high- Previous Day low)).  
As soon as a target/Stop Loss is met, it counts as 1 trade, and the trade status becomes "inactive". A Buy signal can only be generated if the trade status is "inactive".
