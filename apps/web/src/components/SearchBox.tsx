'use client';

import { useState, useEffect, useRef } from 'react';
import { useRouter } from 'next/navigation';
import { Search, Loader2 } from 'lucide-react';
import { api, AutocompleteSuggestion } from '@/lib/api';
import { useQuery } from '@tanstack/react-query';

export function SearchBox({ initialQuery = '' }: { initialQuery?: string }) {
  const [query, setQuery] = useState(initialQuery);
  const [isOpen, setIsOpen] = useState(false);
  const router = useRouter();
  const wrapperRef = useRef<HTMLDivElement>(null);

  const { data, isLoading } = useQuery({
    queryKey: ['autocomplete', query],
    queryFn: async () => {
      if (query.length < 2) return { suggestions: [] };
      const res = await api.get<{ suggestions: AutocompleteSuggestion[] }>('/search/suggestions', { params: { q: query } });
      return res.data;
    },
    enabled: query.length >= 2,
  });

  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (wrapperRef.current && !wrapperRef.current.contains(event.target as Node)) {
        setIsOpen(false);
      }
    }
    document.addEventListener('mousedown', handleClickOutside);
    return () => document.removeEventListener('mousedown', handleClickOutside);
  }, []);

  const handleSearch = (searchQuery: string) => {
    setIsOpen(false);
    if (!searchQuery.trim()) return;
    router.push(`/search?q=${encodeURIComponent(searchQuery)}`);
  };

  const handleKeyDown = (e: React.KeyboardEvent) => {
    if (e.key === 'Enter') {
      handleSearch(query);
    }
  };

  return (
    <div ref={wrapperRef} className="relative w-full max-w-2xl mx-auto z-50">
      <div className="relative group">
        <div className="absolute inset-y-0 left-0 pl-4 flex items-center pointer-events-none">
          <Search className="h-5 w-5 text-muted-foreground group-focus-within:text-primary transition-colors" />
        </div>
        <input
          type="text"
          value={query}
          onChange={(e) => {
            setQuery(e.target.value);
            setIsOpen(true);
          }}
          onKeyDown={handleKeyDown}
          onFocus={() => setIsOpen(true)}
          className="block w-full pl-12 pr-4 py-4 bg-card border border-border rounded-xl leading-5 text-foreground placeholder-muted-foreground focus:outline-none focus:ring-2 focus:ring-primary/50 focus:border-primary transition-all shadow-lg"
          placeholder="Search algorithms, topics, or problems (e.g., 'dijkstra shortest path')"
        />
        <div className="absolute inset-y-0 right-0 pr-4 flex items-center">
          {isLoading && <Loader2 className="h-5 w-5 animate-spin text-muted-foreground" />}
        </div>
      </div>

      {isOpen && data?.suggestions && data.suggestions.length > 0 && (
        <div className="absolute mt-2 w-full bg-card border border-border rounded-xl shadow-2xl overflow-hidden animate-in fade-in slide-in-from-top-2">
          <ul className="py-2">
            {data.suggestions.map((suggestion, idx) => (
              <li
                key={idx}
                onClick={() => {
                  setQuery(suggestion.term);
                  handleSearch(suggestion.term);
                }}
                className="px-4 py-3 hover:bg-secondary cursor-pointer flex items-center justify-between transition-colors"
              >
                <div className="flex items-center gap-3">
                  <Search className="h-4 w-4 text-muted-foreground" />
                  <span className="font-medium text-foreground">{suggestion.term}</span>
                </div>
                <span className="text-xs px-2 py-1 bg-secondary rounded-md text-muted-foreground capitalize">
                  {suggestion.category}
                </span>
              </li>
            ))}
          </ul>
        </div>
      )}
    </div>
  );
}
