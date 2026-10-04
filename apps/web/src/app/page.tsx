import { SearchBox } from '@/components/SearchBox';
import { Activity, Brain, Zap, GitGraph } from 'lucide-react';
import Link from 'next/link';

export default function Home() {
  return (
    <main className="flex min-h-screen flex-col items-center justify-center p-6 lg:p-24 bg-background relative overflow-hidden">
      
      {/* Background gradients */}
      <div className="absolute top-[-10%] left-[-10%] w-[40%] h-[40%] rounded-full bg-primary/20 blur-[120px] pointer-events-none" />
      <div className="absolute bottom-[-10%] right-[-10%] w-[40%] h-[40%] rounded-full bg-blue-500/20 blur-[120px] pointer-events-none" />

      <div className="w-full max-w-5xl text-center space-y-8 z-10">
        
        <div className="inline-flex items-center gap-2 px-3 py-1 rounded-full bg-secondary/50 border border-border text-sm text-muted-foreground mb-4">
          <Zap className="h-4 w-4 text-primary" />
          <span>Powered by custom C++ Engine</span>
        </div>

        <h1 className="text-5xl lg:text-7xl font-extrabold tracking-tight text-transparent bg-clip-text bg-gradient-to-r from-foreground to-foreground/70">
          Search Smarter.<br />Solve Better.
        </h1>
        
        <p className="text-lg lg:text-xl text-muted-foreground max-w-2xl mx-auto">
          Intelligent discovery, search, and recommendation platform for Data Structures and Algorithms. Find exactly what you need to level up your coding skills.
        </p>

        <div className="pt-8 pb-12 w-full max-w-3xl mx-auto">
          <SearchBox />
        </div>

        <div className="grid grid-cols-1 md:grid-cols-3 gap-6 pt-12">
          <FeatureCard 
            icon={<Brain className="h-6 w-6 text-primary" />}
            title="Semantic Search"
            description="Find problems using natural keywords and typos with our custom TF-IDF & Levenshtein matching engine."
          />
          <FeatureCard 
            icon={<GitGraph className="h-6 w-6 text-blue-500" />}
            title="Graph Recommendations"
            description="Discover related topics and algorithms through intelligent BFS/DFS traversal of concept relationships."
          />
          <FeatureCard 
            icon={<Activity className="h-6 w-6 text-emerald-500" />}
            title="Real-time Analytics"
            description="Instant Top-K retrieval using Min-Heaps ensures blazing fast search results under 10ms."
          />
        </div>

        <div className="pt-16 flex flex-wrap justify-center gap-4 text-sm">
          <span className="text-muted-foreground">Popular right now:</span>
          {['dijkstra', 'dynamic programming', 'two sum', 'binary search', 'graph traversal'].map(term => (
            <Link key={term} href={`/search?q=${encodeURIComponent(term)}`} className="text-primary hover:underline underline-offset-4">
              {term}
            </Link>
          ))}
        </div>

      </div>
    </main>
  );
}

function FeatureCard({ icon, title, description }: { icon: React.ReactNode, title: string, description: string }) {
  return (
    <div className="flex flex-col items-center text-center p-6 rounded-2xl bg-card border border-border shadow-lg transition-transform hover:-translate-y-1 duration-300">
      <div className="p-3 rounded-xl bg-secondary mb-4">
        {icon}
      </div>
      <h3 className="font-semibold text-lg mb-2 text-foreground">{title}</h3>
      <p className="text-muted-foreground text-sm leading-relaxed">{description}</p>
    </div>
  );
}
