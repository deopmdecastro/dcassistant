import { useState } from 'react';
import {
  ArrowLeft,
  ArrowRight,
  Bell,
  Bookmark,
  CalendarDays,
  ChevronRight,
  CloudSun,
  Code2,
  FileText,
  FolderOpen,
  Gauge,
  Headphones,
  HeartPulse,
  LayoutGrid,
  Mail,
  Mic,
  Mic2,
  MoreHorizontal,
  PenLine,
  Play,
  Search,
  Settings,
  Shield,
  Sparkles,
  TrendingUp,
  UserRound,
  Waves,
  X,
} from 'lucide-react';
import type { LucideIcon } from 'lucide-react';

type Screen = 'home' | 'chat' | 'voice' | 'apps';
type Topic = { label: string; icon: LucideIcon };
type AppItem = { label: string; icon: LucideIcon; color: string };

const topics: Topic[] = [
  { label: 'Finance', icon: TrendingUp },
  { label: 'Health', icon: Shield },
  { label: 'Develop', icon: Code2 },
  { label: 'Business', icon: LayoutGrid },
];

const apps: AppItem[] = [
  { label: 'Spotify', icon: Headphones, color: '#65d391' },
  { label: 'Agenda', icon: CalendarDays, color: '#77a7ff' },
  { label: 'Gestor', icon: FolderOpen, color: '#ffcc67' },
  { label: 'E-mail', icon: Mail, color: '#ff8b9e' },
  { label: 'Notas', icon: FileText, color: '#f6b7ff' },
  { label: 'Clima', icon: CloudSun, color: '#76d8ef' },
  { label: 'Relatórios', icon: TrendingUp, color: '#70dbb1' },
  { label: 'Config.', icon: Settings, color: '#d3c7e7' },
];

const screenLabels: Record<Screen, string> = {
  home: 'Home',
  chat: 'Chat',
  voice: 'Voice',
  apps: 'Apps',
};

function App() {
  const [screen, setScreen] = useState<Screen>('home');
  const [activeTab, setActiveTab] = useState('Profile');
  const [listening, setListening] = useState(false);
  const [selectedApp, setSelectedApp] = useState<string | null>(null);

  const navigate = (nextScreen: Screen) => {
    setScreen(nextScreen);
    setListening(false);
  };

  return (
    <main className="site-shell">
      <div className="ambient ambient-one" />
      <div className="ambient ambient-two" />
      <section className="workbench">
        <aside className="brand-rail">
          <div className="brand-mark"><Sparkles size={18} strokeWidth={2.5} /></div>
          <div className="brand-copy"><span>DC</span><strong>assistant</strong></div>
          <div className="rail-line" />
          <div className="rail-status"><span className="status-dot" />online</div>
        </aside>

        <section className="device-frame" aria-label="DC Assistant interface preview">
          <div className="device-bezel device-bezel-left" />
          <div className="device-bezel device-bezel-right" />
          <div className="device-screen">
            <ScreenContent
              screen={screen}
              activeTab={activeTab}
              listening={listening}
              selectedApp={selectedApp}
              onTabChange={setActiveTab}
              onNavigate={navigate}
              onListeningChange={setListening}
              onAppSelect={setSelectedApp}
            />
            <BottomNav screen={screen} onNavigate={navigate} />
          </div>
        </section>

        <div className="device-meta">
          <span>DC 0.3</span>
          <span className="meta-divider" />
          <span>ESP32-S3</span>
        </div>
      </section>
      <p className="page-caption">A calm interface for a more capable everyday.</p>
    </main>
  );
}

function ScreenContent({
  screen,
  activeTab,
  listening,
  selectedApp,
  onTabChange,
  onNavigate,
  onListeningChange,
  onAppSelect,
}: {
  screen: Screen;
  activeTab: string;
  listening: boolean;
  selectedApp: string | null;
  onTabChange: (tab: string) => void;
  onNavigate: (screen: Screen) => void;
  onListeningChange: (listening: boolean) => void;
  onAppSelect: (app: string | null) => void;
}) {
  if (screen === 'home') {
    return (
      <div className="screen-content home-screen">
        <div className="topbar">
          <div className="micro-label"><span className="tiny-dot" />DC ASSISTANT</div>
          <div className="tabs" role="tablist" aria-label="Profile navigation">
            {['Profile', 'Analytics', 'Shop'].map((tab) => (
              <button key={tab} className={activeTab === tab ? 'tab active' : 'tab'} onClick={() => onTabChange(tab)}>{tab}</button>
            ))}
          </div>
        </div>
        <div className="home-hero">
          <div className="hero-copy">
            <p className="eyebrow">A new kind of presence</p>
            <h1>Meet the<br /><span>Echo Mind.</span></h1>
            <p className="hero-description">Your thoughtful companion for ideas, focus and the small moments in between.</p>
            <button className="primary-button" onClick={() => onNavigate('chat')}>Get started <ArrowRight size={15} /></button>
          </div>
          <div className="portrait-orbit">
            <div className="orbit orbit-one" />
            <div className="orbit orbit-two" />
            <div className="portrait-glow" />
            <div className="portrait"><UserRound size={76} strokeWidth={1.25} /></div>
            <div className="thought-bubble">Need our<br /><strong>help now?</strong><span /></div>
          </div>
        </div>
        <div className="home-footer"><span>Always here when you need it</span><div className="system-readout"><span>100 FPS</span><span>87% CPU</span></div></div>
      </div>
    );
  }

  if (screen === 'chat') {
    return (
      <div className="screen-content inner-screen">
        <div className="user-header"><div className="user-profile"><div className="mini-avatar"><UserRound size={17} /></div><span>Mark Jonson!</span></div><button className="icon-button"><Bell size={16} /></button></div>
        <div className="chat-layout">
          <div className="chat-actions">
            <button className="feature-card" onClick={() => onNavigate('voice')}><span className="feature-icon"><FileText size={22} /></span><span>Chat with<br />Echo Mind</span><ChevronRight size={14} className="card-arrow" /></button>
            <button className="feature-card" onClick={() => onNavigate('voice')}><span className="feature-icon"><Mic2 size={22} /></span><span>Talk with<br />Echo Mind</span><ChevronRight size={14} className="card-arrow" /></button>
          </div>
          <div className="history-card"><div className="card-heading"><span>History</span><button>View All</button></div><HistoryItem title="How to create IoT app user flow?" date="19/02/2024" icon={Mic2} /><HistoryItem title="Top feature for blockchain app?" date="08/02/2024" icon={FileText} /></div>
        </div>
        <div className="section-heading"><span>Topics</span><button>View All</button></div>
        <div className="topic-grid">{topics.map(({ label, icon: Icon }) => <button className="topic-item" key={label}><span className="topic-icon"><Icon size={18} /></span><span>{label}</span></button>)}</div>
      </div>
    );
  }

  if (screen === 'voice') {
    return (
      <div className="screen-content voice-screen">
        <div className="voice-header"><button className="round-button" onClick={() => onNavigate('chat')}><ArrowLeft size={16} /></button><span>Voice Assessment</span><button className="round-button"><MoreHorizontal size={16} /></button></div>
        <div className="voice-body"><p className="voice-kicker">{listening ? "I'm listening..." : 'Go ahead, I\'m listening...'}</p><div className={listening ? 'wave-stage listening' : 'wave-stage'}><div className="wave wave-a" /><div className="wave wave-b" /><div className="wave wave-c" /><div className="wave-core"><Waves size={34} /></div></div><p className="voice-description">Fluid organic shapes dance across the canvas,<br />conveying a sense of harmony in this<br /><em>abstract design.</em></p></div>
        <div className="voice-controls"><button className="round-button"><PenLine size={16} /></button><button className={listening ? 'mic-button active' : 'mic-button'} onClick={() => onListeningChange(!listening)} aria-label={listening ? 'Stop listening' : 'Start listening'}>{listening ? <X size={26} /> : <Mic size={26} />}</button><button className="round-button"><Bookmark size={16} /></button></div>
      </div>
    );
  }

  return (
    <div className="screen-content inner-screen apps-screen">
      <div className="apps-header"><div><p className="eyebrow">Your workspace</p><h2>Apps</h2></div><div className="header-actions"><button className="round-button"><Search size={16} /></button><button className="round-button"><Settings size={16} /></button></div></div>
      <div className="app-grid">{apps.map(({ label, icon: Icon, color }) => <button key={label} className={selectedApp === label ? 'app-item selected' : 'app-item'} onClick={() => onAppSelect(selectedApp === label ? null : label)}><span className="app-icon" style={{ '--app-color': color } as React.CSSProperties}><Icon size={25} /></span><span>{label}</span></button>)}</div>
      {selectedApp && <div className="app-toast"><span>{selectedApp} selected</span><button onClick={() => onAppSelect(null)}><X size={14} /></button></div>}
      <div className="pagination"><span className="current" /><span /><span /></div>
    </div>
  );
}

function HistoryItem({ title, date, icon: Icon }: { title: string; date: string; icon: LucideIcon }) {
  return <div className="history-item"><span className="history-icon"><Icon size={14} /></span><div><p>{title}</p><small><CalendarDays size={10} />{date}</small></div></div>;
}

function BottomNav({ screen, onNavigate }: { screen: Screen; onNavigate: (screen: Screen) => void }) {
  return <nav className="bottom-nav" aria-label="Screen navigation">{(['home', 'chat', 'voice', 'apps'] as Screen[]).map((item, index) => <button key={item} className={screen === item ? 'nav-dot active' : 'nav-dot'} onClick={() => onNavigate(item)} aria-label={`Go to ${screenLabels[item]}`}>{index + 1}</button>)}</nav>;
}

export default App;
