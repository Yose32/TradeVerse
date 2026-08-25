export interface User {
  userId: string;
  username: string;
  email: string;
  fullName: string;
  cashBalance: number;
  isAdmin: boolean;
}

export interface Stock {
  symbol: string;
  companyName: string;
  sector: string;
  currentPrice: number;
  openPrice: number;
  highPrice: number;
  lowPrice: number;
  previousClose: number;
  change: number;
  changePercent: number;
  volume: number;
  marketCap: number;
}

export interface Candle {
  timestamp: number;
  open: number;
  high: number;
  low: number;
  close: number;
  volume: number;
}

export interface PriceLevel {
  price: number;
  quantity: number;
  orderCount: number;
}

export interface OrderBookDepth {
  symbol: string;
  lastPrice: number;
  bestBid: number;
  bestAsk: number;
  spread: number;
  bids: PriceLevel[];
  asks: PriceLevel[];
}

export interface Order {
  orderId: string;
  userId?: string;
  symbol: string;
  type: 'MARKET' | 'LIMIT';
  side: 'BUY' | 'SELL';
  quantity: number;
  filledQuantity: number;
  remainingQuantity: number;
  price: number;
  status: 'PENDING' | 'PARTIALLY_FILLED' | 'FILLED' | 'CANCELLED' | 'REJECTED';
  timestamp: number;
}

export interface Trade {
  tradeId: string;
  symbol: string;
  side: 'BUY' | 'SELL';
  quantity: number;
  price: number;
  totalValue: number;
  timestamp: number;
}

export interface Holding {
  symbol: string;
  quantity: number;
  averageBuyPrice: number;
  currentPrice: number;
  investedValue: number;
  marketValue: number;
  unrealizedPnL: number;
  pnlPercent: number;
}

export interface Portfolio {
  userId: string;
  cashBalance: number;
  investedValue: number;
  marketValue: number;
  totalPortfolioValue: number;
  unrealizedPnL: number;
  realizedPnL: number;
  totalPnL: number;
  returnPercent: number;
  holdings: Holding[];
}

export interface LeaderboardUser {
  rank: number;
  userId: string;
  username: string;
  fullName: string;
  portfolioValue: number;
  totalPnL: number;
  returnPercent: number;
}

export interface GraphNode {
  name: string;
}

export interface GraphEdge {
  source: string;
  target: string;
  weight: number;
  relationship: string;
}

export interface MarketGraphData {
  vertices: string[];
  edges: GraphEdge[];
}

export interface GraphStepLog {
  step: number;
  currentNode: string;
  description: string;
}

export interface DijkstraResult {
  algorithm: string;
  source: string;
  target: string;
  distance: number;
  path: string[];
  steps: GraphStepLog[];
}

export interface SortStepLog {
  step: number;
  description: string;
  array: number[];
  highlighted: number[];
}

export interface SortTraceResult {
  algorithm: string;
  steps: SortStepLog[];
}

export interface OrderResponse {
  success: boolean;
  orderId: string;
  status: string;
  filledQuantity: number;
  remainingQuantity: number;
  tradesExecuted: number;
  dsaTrace: string[];
  error?: string;
}