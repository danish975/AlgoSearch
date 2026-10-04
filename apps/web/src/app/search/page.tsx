'use client';

import { useQuery } from '@tanstack/react-query';
import { useSearchParams } from 'next/navigation';
import { api, SearchResponse } from '@/lib/api';
import { SearchBox } from '@/components/SearchBox';
import Link from 'next/link';
import { Clock, BarChart, BookOpen, ExternalLink } from 'lucide-react';
import { Suspense } from 'react';

function SearchResultsContent() {
  const searchParams = useSearchParams();
  const q = searchParams.get('q') || '';
  const diff = searchParams.get('difficulty') || '';

  const { data, isLoading, error } = useQuery({
    queryKey: ['search', q, diff],
    queryFn: async () => {
      const res = await api.get<SearchResponse>('/search', { params: { q, difficulty: diff } });
      return res.data;
    },
  });

  return (
    <div className="min-h-screen bg-background">
      <header className="sticky top-0 z-40 w-full border-b border-border bg-background/80 backdrop-blur-md">
        <div className="container mx-auto px-4 h-16 flex items-center justify-between gap-4">
          <Link href="/" className="font-bold text-xl tracking-tight shrink-0">
            Algo<span className="text-primary">Search</span>
          </Link>
          <div className="flex-1 max-w-2xl hidden md:block">
            <SearchBox initialQuery={q} />
          </div>
          <div className="w-20" /> {/* Spacer */}
        </div>
        <div className="md:hidden px-4 pb-4">
          <SearchBox initialQuery={q} />
        </div>
      </header>

      <main className="container mx-auto px-4 py-8 max-w-5xl">
        <div className="flex items-center justify-between mb-8">
          <h1 className="text-2xl font-semibold">
            {q ? (
              <>Results for <span className="text-primary">"{q}"</span></>
            ) : (
              "All Problems"
            )}
          </h1>
          {data && (
            <div className="text-sm text-muted-foreground flex items-center gap-2 bg-secondary px-3 py-1 rounded-full">
              <Clock className="w-4 h-4" />
              {data.total_count} results in {data.search_duration_ms.toFixed(2)}ms
            </div>
          )}
        </div>

        {isLoading && (
          <div className="space-y-4">
            {[1, 2, 3, 4].map(i => (
              <div key={i} className="h-32 w-full bg-card animate-pulse rounded-xl border border-border" />
            ))}
          </div>
        )}

        {error && (
          <div className="p-8 text-center border border-destructive/20 bg-destructive/10 rounded-xl text-destructive">
            Failed to load search results. Please ensure the backend and C++ engine are running.
          </div>
        )}

        {data && data.results.length === 0 && (
          <div className="py-20 text-center">
            <div className="inline-flex p-4 rounded-full bg-secondary mb-4">
              <Search className="w-8 h-8 text-muted-foreground" />
            </div>
            <h2 className="text-xl font-semibold mb-2">No results found</h2>
            <p className="text-muted-foreground">Try adjusting your search terms or filters.</p>
          </div>
        )}

        {data && data.results.length > 0 && (
          <div className="space-y-4">
            {data.results.map((result) => (
              <div key={result.problem.id} className="group relative bg-card hover:bg-secondary/50 border border-border rounded-xl p-5 transition-all shadow-sm hover:shadow-md">
                <div className="flex items-start justify-between gap-4">
                  <div className="space-y-2 flex-1">
                    <Link href={`/problems/${result.problem.id}`} className="text-xl font-semibold text-foreground hover:text-primary transition-colors flex items-center gap-2">
                      {result.problem.title}
                    </Link>
                    
                    <div className="flex flex-wrap gap-2 items-center text-xs mt-2">
                      <span className={`px-2.5 py-1 rounded-md font-medium
                        ${result.problem.difficulty === 'Easy' ? 'bg-emerald-500/10 text-emerald-500 border border-emerald-500/20' : 
                          result.problem.difficulty === 'Medium' ? 'bg-amber-500/10 text-amber-500 border border-amber-500/20' : 
                          'bg-rose-500/10 text-rose-500 border border-rose-500/20'}`}>
                        {result.problem.difficulty}
                      </span>
                      {result.problem.topics.slice(0, 3).map(topic => (
                        <span key={topic} className="px-2 py-1 bg-secondary rounded-md text-muted-foreground border border-border">
                          {topic}
                        </span>
                      ))}
                    </div>

                    <p className="text-muted-foreground text-sm line-clamp-2 mt-3">
                      {result.problem.description}
                    </p>
                  </div>

                  <div className="hidden sm:flex flex-col items-end gap-2 shrink-0 text-xs">
                    <div className="px-2 py-1 bg-primary/10 text-primary rounded-md border border-primary/20 flex items-center gap-1 font-mono">
                      <BarChart className="w-3 h-3" />
                      Score: {(result.score * 100).toFixed(0)}
                    </div>
                    {result.problem.externalUrl && (
                      <a href={result.problem.externalUrl} target="_blank" rel="noreferrer" className="text-muted-foreground hover:text-foreground flex items-center gap-1 transition-colors">
                        View on {result.problem.platform} <ExternalLink className="w-3 h-3" />
                      </a>
                    )}
                  </div>
                </div>
              </div>
            ))}
          </div>
        )}
      </main>
    </div>
  );
}

export default function SearchPage() {
  return (
    <Suspense fallback={<div>Loading search...</div>}>
      <SearchResultsContent />
    </Suspense>
  );
}
