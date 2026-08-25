import React, { useState } from 'react';
import { AuthProvider, useAuth } from './context/AuthContext';
import { TradingProvider, useTrading } from './context/TradingContext';
import { Navbar } from './components/common/Navbar';
import { TickerTape } from './components/common/TickerTape';
import { AIAssistantModal } from './components/ai/AIAssistantModal';
import { AdminDemoModal } from './components/admin/AdminDemoModal';

import { LandingPage } from './pages/LandingPage';
import { LoginPage } from './pages/LoginPage';
import { SignUpPage } from './pages/SignUpPage';
import { DashboardPage } from './pages/DashboardPage';
import { MarketsPage } from './pages/MarketsPage';
import { StockDetailPage } from './pages/StockDetailPage';
import { PortfolioPage } from './pages/PortfolioPage';
import { OrdersPage } from './pages/OrdersPage';
import { HistoryPage } from './pages/HistoryPage';
import { LeaderboardPage } from './pages/LeaderboardPage';
import { DSALabPage } from './pages/DSALabPage';
import { Stock } from './types';

const MainApp: React.FC = () => {
  const { isAuthenticated, isLoading } = useAuth();
  const { selectedStock, setSelectedStock } = useTrading();

  const [activeTab, setActiveTab] = useState<string>('landing');
  const [isAIOpen, setIsAIOpen] = useState<boolean>(false);
  const [isAdminOpen, setIsAdminOpen] = useState<boolean>(false);

  // Auto-switch to dashboard when authenticated on landing
  React.useEffect(() => {
    if (isAuthenticated && (activeTab === 'landing' || activeTab === 'login' || activeTab === 'signup')) {
      setActiveTab('dashboard');
    }
  }, [isAuthenticated]);

  const handleSelectStock = (stock: Stock) => {
    setSelectedStock(stock);
    setActiveTab('stock_detail');
  };

  if (isLoading) {
    return (
      <div className="min-h-screen bg-[#080d1a] flex items-center justify-center text-slate-400 font-mono text-sm">
        Initializing TradeVerse C++ Trading Engine...
      </div>
    );
  }

  return (
    <div className="min-h-screen flex flex-col bg-[#080d1a] text-slate-100">
      {/* Global Top Navbar */}
      <Navbar
        activeTab={activeTab}
        setActiveTab={setActiveTab}
        onOpenAI={() => setIsAIOpen(true)}
        onOpenAdmin={() => setIsAdminOpen(true)}
      />

      {/* Live Market Ticker Tape */}
      <TickerTape />

      {/* Main Content Area */}
      <main className="flex-1 pb-12">
        {activeTab === 'landing' && (
          <LandingPage
            onExplore={() => setActiveTab('markets')}
            onLogin={() => setActiveTab('login')}
            onSignUp={() => setActiveTab('signup')}
          />
        )}
        {activeTab === 'login' && (
          <LoginPage
            onSuccess={() => setActiveTab('dashboard')}
            onNavigateSignUp={() => setActiveTab('signup')}
          />
        )}
        {activeTab === 'signup' && (
          <SignUpPage
            onSuccess={() => setActiveTab('dashboard')}
            onNavigateLogin={() => setActiveTab('login')}
          />
        )}
        {activeTab === 'dashboard' && (
          <DashboardPage
            onSelectStock={handleSelectStock}
            onNavigateMarkets={() => setActiveTab('markets')}
          />
        )}
        {activeTab === 'markets' && (
          <MarketsPage onSelectStock={handleSelectStock} />
        )}
        {activeTab === 'stock_detail' && selectedStock && (
          <StockDetailPage
            stock={selectedStock}
            onBack={() => setActiveTab('markets')}
          />
        )}
        {activeTab === 'portfolio' && (
          <PortfolioPage onSelectStock={handleSelectStock} />
        )}
        {activeTab === 'orders' && <OrdersPage />}
        {activeTab === 'history' && <HistoryPage />}
        {activeTab === 'leaderboard' && <LeaderboardPage />}
        {activeTab === 'dsa_lab' && <DSALabPage />}
      </main>

      {/* Interactive AI Assistant Modal */}
      <AIAssistantModal isOpen={isAIOpen} onClose={() => setIsAIOpen(false)} />

      {/* Admin / Demo Controller Modal */}
      <AdminDemoModal isOpen={isAdminOpen} onClose={() => setIsAdminOpen(false)} />
    </div>
  );
};

export const App: React.FC = () => {
  return (
    <AuthProvider>
      <TradingProvider>
        <MainApp />
      </TradingProvider>
    </AuthProvider>
  );
};

export default App;