'use client';

import { useQuery } from '@tanstack/react-query';
import { api, Problem } from '@/lib/api';
import Link from 'next/link';
import { ArrowLeft, ExternalLink, BookmarkPlus, GitGraph } from 'lucide-react';

export default function ProblemPage({ params }: { params: { id: string } }) {
  const { data: problem, isLoading } = useQuery({
    queryKey: ['problem', params.id],
    queryFn: async () => {
      const res = await api.get<Problem>(`/problems/${params.id}`);
      return res.data;
    },
  });

  const { data: related } = useQuery({
    queryKey: ['related', params.id],
    queryFn: async () => {
      const res = await api.get<{ related: { problem: Problem, similarity: number, shared_topics: string[] }[] }>(`/problems/${params.id}/related`);
      return res.data.related;
    },
    enabled: !!problem,
  });

  if (isLoading) {
    return (
      <div className="container mx-auto p-8 max-w-4xl space-y-4">
        <div className="h-8 w-32 bg-card animate-pulse rounded-md" />
        <div className="h-12 w-3/4 bg-card animate-pulse rounded-md mt-8" />
        <div className="h-64 w-full bg-card animate-pulse rounded-md mt-4" />
      </div>
    );
  }

  if (!problem) {
    return (
      <div className="container mx-auto p-8 text-center text-muted-foreground">
        Problem not found.
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-background">
      <header className="sticky top-0 z-40 w-full border-b border-border bg-background/80 backdrop-blur-md">
        <div className="container mx-auto px-4 h-16 flex items-center gap-4">
          <Link href="/search" className="text-muted-foreground hover:text-foreground transition-colors flex items-center gap-2">
            <ArrowLeft className="w-4 h-4" /> Back
          </Link>
          <div className="h-4 w-px bg-border mx-2" />
          <span className="font-medium truncate">{problem.title}</span>
        </div>
      </header>

      <main className="container mx-auto px-4 py-8 max-w-6xl grid grid-cols-1 lg:grid-cols-3 gap-8">
        
        {/* Left Column: Details */}
        <div className="lg:col-span-2 space-y-8">
          <div>
            <div className="flex items-start justify-between gap-4 mb-4">
              <h1 className="text-3xl font-bold tracking-tight">{problem.title}</h1>
              <button className="flex items-center gap-2 px-3 py-1.5 rounded-md bg-secondary text-sm font-medium hover:bg-secondary/80 transition-colors">
                <BookmarkPlus className="w-4 h-4" /> Save
              </button>
            </div>
            
            <div className="flex flex-wrap items-center gap-3 text-sm mb-8">
              <span className={`px-2.5 py-1 rounded-md font-medium border
                ${problem.difficulty === 'Easy' ? 'bg-emerald-500/10 text-emerald-500 border-emerald-500/20' : 
                  problem.difficulty === 'Medium' ? 'bg-amber-500/10 text-amber-500 border-amber-500/20' : 
                  'bg-rose-500/10 text-rose-500 border-rose-500/20'}`}>
                {problem.difficulty}
              </span>
              <span className="text-muted-foreground px-2 py-1 bg-secondary rounded-md border border-border">
                {problem.platform}
              </span>
              {problem.externalUrl && (
                <a href={problem.externalUrl} target="_blank" rel="noreferrer" className="text-primary hover:underline flex items-center gap-1 px-2 py-1">
                  View Source <ExternalLink className="w-3 h-3" />
                </a>
              )}
            </div>

            <div className="prose prose-invert max-w-none text-foreground/90">
              <h3 className="text-xl font-semibold mb-4 text-foreground">Description</h3>
              <p className="leading-relaxed whitespace-pre-wrap">{problem.description}</p>
            </div>
          </div>

          <div className="grid sm:grid-cols-2 gap-6 pt-8 border-t border-border">
            <div>
              <h3 className="text-sm font-medium text-muted-foreground mb-3">Topics</h3>
              <div className="flex flex-wrap gap-2">
                {problem.topics.map(topic => (
                  <Link key={topic} href={`/search?topic=${encodeURIComponent(topic)}`} className="px-3 py-1.5 bg-secondary hover:bg-secondary/80 border border-border rounded-lg text-sm transition-colors">
                    {topic}
                  </Link>
                ))}
              </div>
            </div>
            <div>
              <h3 className="text-sm font-medium text-muted-foreground mb-3">Algorithms</h3>
              <div className="flex flex-wrap gap-2">
                {problem.algorithms.map(algo => (
                  <Link key={algo} href={`/search?algorithm=${encodeURIComponent(algo)}`} className="px-3 py-1.5 bg-primary/10 hover:bg-primary/20 text-primary border border-primary/20 rounded-lg text-sm transition-colors">
                    {algo}
                  </Link>
                ))}
              </div>
            </div>
          </div>
        </div>

        {/* Right Column: Recommendations */}
        <div className="space-y-6">
          <div className="bg-card border border-border rounded-xl p-5 shadow-sm">
            <div className="flex items-center gap-2 mb-4 text-foreground font-semibold">
              <GitGraph className="w-5 h-5 text-primary" />
              <h2>Related Problems</h2>
            </div>
            
            {!related && (
              <div className="space-y-3">
                {[1, 2, 3].map(i => <div key={i} className="h-16 bg-secondary/50 animate-pulse rounded-lg" />)}
              </div>
            )}

            {related && related.length === 0 && (
              <p className="text-sm text-muted-foreground">No related problems found.</p>
            )}

            {related && related.length > 0 && (
              <div className="space-y-3">
                {related.map(r => (
                  <Link key={r.problem.id} href={`/problems/${r.problem.id}`} className="block p-3 rounded-lg hover:bg-secondary/80 border border-transparent hover:border-border transition-colors">
                    <div className="font-medium text-sm mb-1 truncate">{r.problem.title}</div>
                    <div className="flex items-center gap-2 text-xs">
                      <span className="text-muted-foreground">Match: {(r.similarity * 100).toFixed(0)}%</span>
                      {r.shared_topics.length > 0 && (
                        <>
                          <span className="w-1 h-1 rounded-full bg-border" />
                          <span className="text-primary truncate">{r.shared_topics[0]}</span>
                        </>
                      )}
                    </div>
                  </Link>
                ))}
              </div>
            )}
          </div>
        </div>

      </main>
    </div>
  );
}
