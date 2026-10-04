export declare const searchEngineService: {
    search(params: any): Promise<any>;
    autocomplete(q: string, limit?: number): Promise<any>;
    getProblem(id: string): Promise<any>;
    getRelatedProblems(id: string, limit?: number): Promise<any>;
    getTopics(): Promise<any>;
    getAlgorithms(): Promise<any>;
};
//# sourceMappingURL=searchEngine.d.ts.map