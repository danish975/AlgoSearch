import type { Metadata } from 'next';
import { Inter, Geist } from 'next/font/google';
import './globals.css';
import { ThemeProvider } from '@/components/ThemeProvider';
import { QueryProvider } from '@/components/QueryProvider';
import { cn } from "@/lib/utils";

const geist = Geist({subsets:['latin'],variable:'--font-sans'});

const inter = Inter({ subsets: ['latin'] });

export const metadata: Metadata = {
  title: 'AlgoSearch — Intelligent DSA Search & Recommendation Platform',
  description: 'Search smarter. Solve better. Find and understand algorithms with our custom C++ search engine.',
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en" className={cn("dark", "font-sans", geist.variable)} suppressHydrationWarning>
      <body className={`${inter.className} min-h-screen bg-background text-foreground antialiased`}>
        <ThemeProvider attribute="class" defaultTheme="dark" enableSystem={false}>
          <QueryProvider>
            {children}
          </QueryProvider>
        </ThemeProvider>
      </body>
    </html>
  );
}
