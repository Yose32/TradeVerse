import {
  User, Stock, OrderBookDepth, Candle, Order, Trade, Portfolio,
  LeaderboardUser, MarketGraphData, DijkstraResult, SortTraceResult, OrderResponse
} from '../types';

const API_BASE_URL = 'http://127.0.0.1:8080/api';

class ApiService {
  private token: string | null = null;

  constructor() {
    this.token = localStorage.getItem('tradeverse_token');
  }

  setToken(token: string | null) {
    this.token = token;
    if (token) {
      localStorage.setItem('tradeverse_token', token);
    } else {
      localStorage.removeItem('tradeverse_token');
    }
  }

  getToken(): string | null {
    return this.token;
  }

  private async request<T>(endpoint: string, options: RequestInit = {}): Promise<T> {
    const headers: Record<string, string> = {
      'Content-Type': 'application/json',
      Accept: 'application/json',
      ...(options.headers as Record<string, string>),
    };

    if (this.token) {
      headers['Authorization'] = `Bearer ${this.token}`;
    }

    try {
      const response = await fetch(`${API_BASE_URL}${endpoint}`, {
        ...options,
        headers,
      });

      const data = await response.json();
      if (!response.ok) {
        throw new Error(data.error || `HTTP error! Status: ${response.status}`);
      }
      return data as T;
    } catch (err: any) {
      console.error(`API Error on ${endpoint}:`, err);
      throw err;
    }
  }

  // --- AUTH ---
  async login(identifier: string, password: string): Promise<{ success: boolean; token: string; user: User }> {
    const res = await this.request<{ success: boolean; token: string; user: User }>('/auth/login', {
      method: 'POST',
      body: JSON.stringify({ username: identifier, password }),
    });
    if (res.token) {
      this.setToken(res.token);
    }
    return res;
  }

  async register(username: string, email: string, fullName: string, password: string): Promise<{ success: boolean; token: string; user: User }> {
    const res = await this.request<{ success: boolean; token: string; user: User }>('/auth/register', {
      method: 'POST',
      body: JSON.stringify({ username, email, fullName, password }),
    });
    if (res.token) {
      this.setToken(res.token);
    }
    return res;
  }

  async logout(): Promise<void> {
    try {
      await this.request('/auth/logout', { method: 'POST' });
    } catch (e) {
      // ignore
    } finally {
      this.setToken(null);
    }
  }

  async getProfile(): Promise<any> {
    return this.request('/user/profile');
  }

  // --- STOCKS & MARKET ---
  async getStocks(): Promise<Stock[]> {
    return this.request<Stock[]>('/stocks');
  }

  async getStock(symbol: string): Promise<Stock> {
    return this.request<Stock>(`/stocks/${symbol}`);
  }

  async getOrderBook(symbol: string): Promise<OrderBookDepth> {
    return this.request<OrderBookDepth>(`/stocks/${symbol}/orderbook`);
  }

  async getCandles(symbol: string): Promise<Candle[]> {
    return this.request<Candle[]>(`/stocks/${symbol}/candles`);
  }

  // --- TRADING & ORDERS ---
  async placeOrder(params: {
    symbol: string;
    type: 'MARKET' | 'LIMIT';
    side: 'BUY' | 'SELL';
    quantity: number;
    price: number;
  }): Promise<OrderResponse> {
    return this.request<OrderResponse>('/orders', {
      method: 'POST',
      body: JSON.stringify(params),
    });
  }

  async getOrders(): Promise<Order[]> {
    return this.request<Order[]>('/orders');
  }

  async getTrades(): Promise<Trade[]> {
    return this.request<Trade[]>('/trades');
  }

  async getPortfolio(): Promise<Portfolio> {
    return this.request<Portfolio>('/portfolio');
  }

  async getLeaderboard(): Promise<LeaderboardUser[]> {
    return this.request<LeaderboardUser[]>('/leaderboard');
  }

  // --- DSA LAB ---
  async getMarketGraph(): Promise<MarketGraphData> {
    return this.request<MarketGraphData>('/dsa/graph');
  }

  async runBFS(startNode: string): Promise<any> {
    return this.request('/dsa/bfs', {
      method: 'POST',
      body: JSON.stringify({ startNode }),
    });
  }

  async runDFS(startNode: string): Promise<any> {
    return this.request('/dsa/dfs', {
      method: 'POST',
      body: JSON.stringify({ startNode }),
    });
  }

  async runDijkstra(source: string, target: string): Promise<DijkstraResult> {
    return this.request<DijkstraResult>('/dsa/dijkstra', {
      method: 'POST',
      body: JSON.stringify({ source, target }),
    });
  }

  async runSortTrace(algorithm: 'MERGE_SORT' | 'QUICK_SORT'): Promise<SortTraceResult> {
    return this.request<SortTraceResult>('/dsa/sort/trace', {
      method: 'POST',
      body: JSON.stringify({ algorithm }),
    });
  }

  // --- ADMIN & SIMULATION ---
  async setMarketRegime(regime: string): Promise<{ success: boolean; currentRegime: string }> {
    return this.request('/admin/regime', {
      method: 'POST',
      body: JSON.stringify({ regime }),
    });
  }

  async triggerMarketEvent(params: {
    title: string;
    description: string;
    epicentre: string;
    impact: number;
  }): Promise<{ success: boolean; eventId: string; epicentre: string }> {
    return this.request('/admin/events/trigger', {
      method: 'POST',
      body: JSON.stringify(params),
    });
  }

  async triggerTick(): Promise<{ success: boolean }> {
    return this.request('/admin/tick', { method: 'POST' });
  }

  // --- AI ASSISTANT ---
  async askAI(question: string): Promise<{ answer: string }> {
    return this.request<{ answer: string }>('/ai/ask', {
      method: 'POST',
      body: JSON.stringify({ question }),
    });
  }
}

export const api = new ApiService();