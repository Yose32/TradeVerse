import React, { createContext, useContext, useState, useEffect } from 'react';
import { Stock, Portfolio } from '../types';
import { api } from '../services/api';
import { useAuth } from './AuthContext';

interface TradingContextType {
  stocks: Stock[];
  portfolio: Portfolio | null;
  selectedStock: Stock | null;
  setSelectedStock: (s: Stock | null) => void;
  refreshMarketData: () => Promise<void>;
  refreshPortfolio: () => Promise<void>;
}

const TradingContext = createContext<TradingContextType | undefined>(undefined);

export const TradingProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { isAuthenticated } = useAuth();
  const [stocks, setStocks] = useState<Stock[]>([]);
  const [portfolio, setPortfolio] = useState<Portfolio | null>(null);
  const [selectedStock, setSelectedStock] = useState<Stock | null>(null);

  const refreshMarketData = async () => {
    try {
      const data = await api.getStocks();
      setStocks(data);
      if (!selectedStock && data.length > 0) {
        setSelectedStock(data[0]);
      } else if (selectedStock) {
        const updated = data.find((s) => s.symbol === selectedStock.symbol);
        if (updated) setSelectedStock(updated);
      }
    } catch (e) {
      console.error('Failed to fetch stocks', e);
    }
  };

  const refreshPortfolio = async () => {
    if (!isAuthenticated) {
      setPortfolio(null);
      return;
    }
    try {
      const data = await api.getPortfolio();
      setPortfolio(data);
    } catch (e) {
      console.error('Failed to fetch portfolio', e);
    }
  };

  useEffect(() => {
    refreshMarketData();
    const interval = setInterval(() => {
      refreshMarketData();
      if (isAuthenticated) {
        refreshPortfolio();
      }
    }, 1500);
    return () => clearInterval(interval);
  }, [isAuthenticated, selectedStock?.symbol]);

  return (
    <TradingContext.Provider
      value={{
        stocks,
        portfolio,
        selectedStock,
        setSelectedStock,
        refreshMarketData,
        refreshPortfolio,
      }}
    >
      {children}
    </TradingContext.Provider>
  );
};

export const useTrading = (): TradingContextType => {
  const context = useContext(TradingContext);
  if (!context) {
    throw new Error('useTrading must be used within a TradingProvider');
  }
  return context;
};