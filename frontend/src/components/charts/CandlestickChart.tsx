import React, { useState, useEffect, useRef } from 'react';
import { BarChart2, Maximize2, RefreshCw } from 'lucide-react';
import { api } from '../../services/api';
import { Candle, Stock } from '../../types';

interface CandlestickChartProps {
  stock: Stock;
}

export const CandlestickChart: React.FC<CandlestickChartProps> = ({ stock }) => {
  const [timeframe, setTimeframe] = useState<'1D' | '1W' | '1M' | '3M' | '1Y'>('1D');
  const [candles, setCandles] = useState<Candle[]>([]);
  const [hoveredCandle, setHoveredCandle] = useState<Candle | null>(null);
  const canvasRef = useRef<HTMLCanvasElement | null>(null);

  useEffect(() => {
    let isMounted = true;
    const fetchCandles = async () => {
      try {
        const data = await api.getCandles(stock.symbol);
        if (isMounted) setCandles(data);
      } catch (e) {
        // silent polling catch
      }
    };

    fetchCandles();
    const interval = setInterval(fetchCandles, 2000);
    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, [stock.symbol]);

  // Render Canvas Candlesticks
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas || candles.length === 0) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const dpr = window.devicePixelRatio || 1;
    const width = canvas.clientWidth;
    const height = canvas.clientHeight;
    canvas.width = width * dpr;
    canvas.height = height * dpr;
    ctx.scale(dpr, dpr);

    ctx.clearRect(0, 0, width, height);

    // Calculate price bounds
    let minPrice = Math.min(...candles.map((c) => c.low));
    let maxPrice = Math.max(...candles.map((c) => c.high));
    const pricePadding = (maxPrice - minPrice) * 0.1 || 1.0;
    minPrice -= pricePadding;
    maxPrice += pricePadding;
    const priceRange = maxPrice - minPrice;

    // Calculate max volume for bottom volume chart
    const maxVolume = Math.max(...candles.map((c) => c.volume), 1);

    // Layout dimensions
    const candleAreaHeight = height * 0.75;
    const volumeAreaHeight = height * 0.20;
    const volumeTop = height * 0.80;

    // Draw grid lines
    ctx.strokeStyle = 'rgba(51, 65, 85, 0.3)';
    ctx.lineWidth = 1;
    ctx.setLineDash([4, 4]);

    for (let i = 1; i <= 4; ++i) {
      const y = (candleAreaHeight / 5) * i;
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(width, y);
      ctx.stroke();

      const priceVal = maxPrice - (priceRange / 5) * i;
      ctx.fillStyle = '#64748b';
      ctx.font = '10px JetBrains Mono';
      ctx.fillText(`₹${priceVal.toFixed(2)}`, width - 55, y - 4);
    }
    ctx.setLineDash([]);

    // Draw candles and volume bars
    const candleCount = candles.length;
    const candleWidth = Math.max(3, (width / candleCount) * 0.65);
    const spacing = width / candleCount;

    candles.forEach((candle, idx) => {
      const isUp = candle.close >= candle.open;
      const color = isUp ? '#10b981' : '#f43f5e';
      const x = idx * spacing + spacing / 2;

      // Price coordinate mapping
      const openY = candleAreaHeight - ((candle.open - minPrice) / priceRange) * candleAreaHeight;
      const closeY = candleAreaHeight - ((candle.close - minPrice) / priceRange) * candleAreaHeight;
      const highY = candleAreaHeight - ((candle.high - minPrice) / priceRange) * candleAreaHeight;
      const lowY = candleAreaHeight - ((candle.low - minPrice) / priceRange) * candleAreaHeight;

      // Draw high-low wick line
      ctx.strokeStyle = color;
      ctx.lineWidth = 1.5;
      ctx.beginPath();
      ctx.moveTo(x, highY);
      ctx.lineTo(x, lowY);
      ctx.stroke();

      // Draw candle body
      const bodyTop = Math.min(openY, closeY);
      const bodyHeight = Math.max(2, Math.abs(closeY - openY));
      ctx.fillStyle = color;
      ctx.fillRect(x - candleWidth / 2, bodyTop, candleWidth, bodyHeight);

      // Draw volume bar
      const volBarHeight = (candle.volume / maxVolume) * volumeAreaHeight;
      const volY = height - volBarHeight;
      ctx.fillStyle = isUp ? 'rgba(16, 185, 129, 0.35)' : 'rgba(244, 63, 94, 0.35)';
      ctx.fillRect(x - candleWidth / 2, volY, candleWidth, volBarHeight);
    });
  }, [candles]);

  const latestCandle = candles[candles.length - 1] || null;
  const displayCandle = hoveredCandle || latestCandle;

  return (
    <div className="p-5 rounded-2xl glass-card border border-slate-800/80 shadow-2xl space-y-4">
      {/* Top Header & Timeframe Buttons */}
      <div className="flex flex-wrap items-center justify-between gap-3 pb-3 border-b border-slate-800/80">
        <div>
          <div className="flex items-center gap-2">
            <h2 className="font-extrabold text-lg text-slate-100">{stock.companyName}</h2>
            <span className="text-xs font-mono font-bold px-2 py-0.5 rounded-md bg-blue-500/10 text-blue-400 border border-blue-500/20">
              {stock.symbol}
            </span>
            <span className="text-xs text-slate-400 font-medium">({stock.sector})</span>
          </div>
          <div className="flex items-center gap-3 mt-1 font-mono text-xs">
            <span className="text-xl font-bold text-slate-100">
              ₹{stock.currentPrice.toFixed(2)}
            </span>
            <span
              className={`font-semibold ${
                stock.change >= 0 ? 'text-emerald-400' : 'text-rose-400'
              }`}
            >
              {stock.change >= 0 ? '+' : ''}₹{stock.change.toFixed(2)} ({stock.change >= 0 ? '+' : ''}
              {stock.changePercent.toFixed(2)}%)
            </span>
          </div>
        </div>

        {/* Timeframe Chips */}
        <div className="flex items-center gap-1 p-1 rounded-xl bg-slate-900 border border-slate-800">
          {(['1D', '1W', '1M', '3M', '1Y'] as const).map((tf) => (
            <button
              key={tf}
              onClick={() => setTimeframe(tf)}
              className={`px-3 py-1 rounded-lg text-xs font-mono font-semibold transition-all ${
                timeframe === tf
                  ? 'bg-blue-600 text-white shadow-sm'
                  : 'text-slate-400 hover:text-slate-200'
              }`}
            >
              {tf}
            </button>
          ))}
        </div>
      </div>

      {/* OHLCV Banner */}
      {displayCandle && (
        <div className="flex flex-wrap items-center gap-4 text-xs font-mono text-slate-400 bg-slate-900/60 p-2.5 rounded-xl border border-slate-800/80">
          <div>
            <span className="text-slate-500">O: </span>
            <span className="text-slate-200">₹{displayCandle.open.toFixed(2)}</span>
          </div>
          <div>
            <span className="text-slate-500">H: </span>
            <span className="text-emerald-400">₹{displayCandle.high.toFixed(2)}</span>
          </div>
          <div>
            <span className="text-slate-500">L: </span>
            <span className="text-rose-400">₹{displayCandle.low.toFixed(2)}</span>
          </div>
          <div>
            <span className="text-slate-500">C: </span>
            <span className="text-slate-200 font-bold">₹{displayCandle.close.toFixed(2)}</span>
          </div>
          <div>
            <span className="text-slate-500">Vol: </span>
            <span className="text-slate-300">{displayCandle.volume.toLocaleString()}</span>
          </div>
        </div>
      )}

      {/* Chart Canvas */}
      <div className="relative w-full h-80 rounded-xl bg-[#060a14] border border-slate-900 overflow-hidden">
        <canvas
          ref={canvasRef}
          className="w-full h-full cursor-crosshair"
        />
      </div>
    </div>
  );
};