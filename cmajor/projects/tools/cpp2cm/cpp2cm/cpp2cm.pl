namespace cpp2cm
{
    grammar SolutionGrammar
    {
        Solution : Solution*;
        SolutionHeader : Solution*;
        Projects(Solution* solution);
        Project : Project*;
        ProjectHeader : Project*;
        ProjectSection(Project* project);
        ProjectDependencySection(Project* project);
        ProjectDependencySectionHeader;
        Global;
        GlobalSections;
        GlobalSection;
        SolutionConfigurationPlatformsSection;
        SolutionConfigurationPlatformsSectionHeader;
        SolutionPropertiesSection;
        SolutionPropertiesSectionHeader;
        EntensibilityGlobalsSection;
        EntensibilityGlobalsSectionHeader;
        PerformanceSection;
        PerformanceSectionHeader;
        ConfigurationLine;
        ProjectConfigurationPlatformsSection;
        ProjectConfigurationPlatformsSectionHeader;
        GuidLine : ustring;
        LanguageGuidString : ustring;
        ProjectNameString : ustring;
        ProjectFilePathString : string;
        FilePath : ustring;
        ProjectGuidString : ustring;
        GuidString : ustring;
        Guid : ustring;
        HexDigit4;
        HexDigit8;
        HexDigit12;
        FormatVersion : ustring;
        VisualStudioVersion : ustring;
        MinimumVisualStudioVersion : ustring;
        FullVersion : ustring;
        MajorMinorVersion : ustring;
        VersionNumber;
        SpacesAndComments;
        Comment;
    }
}
